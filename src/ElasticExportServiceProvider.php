<?php

namespace ElasticExport;

use Plenty\Modules\DataExchange\Services\ExportPresetContainer;
use Plenty\Plugin\DataExchangeServiceProvider;
use Plenty\Modules\Item\Search\Contracts\VariationElasticSearchAvailibilityRepositoryContract;

class ElasticExportServiceProvider extends DataExchangeServiceProvider
{
    const RAKUTEN_DE    = 'RakutenDE';
    const BILLIGER_DE   = 'BilligerDE';
    const CDISCOUNT_COM = 'CdiscountCOM';
    const BASICPRICESEARCHENGINE = 'BasicPriceSearchEngine';
    const BEEZUP = 'BeezUp';
    const KAUFLUXDE = 'KaufluxDE';

	public function register()
	{
	}

	public function exports(ExportPresetContainer $container)
	{
		$formats = [
			'CdiscountCOM',
			'Check24DE',
			'BeezUp',
			'BelboonDE',
			'BilligerDE',
			'GeizhalsDE',
			'GoogleShopping',
			'GuenstigerDE',
			'IdealoDE',
			'KaufluxDE',
			'PreisRoboterDE',
			'RakutenDE',
			'ShopzillaDE',
			'ShoppingCOM',
            'BasicPriceSearchEngine',
            'KelkooBasicDE',
            'KelkooPremiumDE',
            'KuponaDE',
            'FashionDE',
            'EcondaDE',
            'MyBestBrandsDE',
            'TreepodiaCOM',
            'TwengaCOM',
            'ZanoxDE',
            'SchuheDE',
			'ShippingProfiles',
            'Shopping24DE',
            'TracdelightCOM',            
		];

        /**
         * @var VariationElasticSearchAvailibilityRepositoryContract $variationElasticSearchAvailabilityRepository
         */
        $variationElasticSearchAvailabilityRepository = pluginApp(VariationElasticSearchAvailibilityRepositoryContract::class);

        $isAvailableForElasticSearch = $variationElasticSearchAvailabilityRepository->isAvailable();
        $esReadyMarketplaces = $this->getEsReadyMarketPlaces();

		foreach ($formats as $format)
		{
            if(is_array($esReadyMarketplaces) && in_array($format, $esReadyMarketplaces) &&
                $isAvailableForElasticSearch === true)
            {
                $container->add(
                    $format,
                    'ElasticExport\ES_ResultFields\\'.$format,
                    'ElasticExport\ES_Generators\\'.$format,
                    'ElasticExport\Filters\\' . $format
                );
            }
            else
            {
                $container->add(
                    $format,
                    'ElasticExport\ResultFields\\'.$format,
                    'ElasticExport\Generators\\'.$format,
                    'ElasticExport\Filters\\' . $format
                );
            }
		}
	}

    /**
     * Get a List of Marketplaceformats which are already adjusted to work with ElasticSearch
     * @return array
     */
	private function getEsReadyMarketPlaces()
    {
        $marketplaces = array(
            self::RAKUTEN_DE,
            self::BILLIGER_DE,
            self::CDISCOUNT_COM,
            self::BASICPRICESEARCHENGINE,
            self::BEEZUP,
            self::KAUFLUXDE,
        );

        return $marketplaces;
    }
}
