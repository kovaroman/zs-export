<?php

namespace ElasticExport\Filters;

use Plenty\Modules\DataExchange\Contracts\FiltersForElasticSearchContract;

use Plenty\Plugin\Application;

use Plenty\Modules\Item\Search\Filter\VariationBaseFilter;

class RakutenDE extends FiltersForElasticSearchContract
{
    /**
     * @var Application $app
     */
    private $app;


    public function __construct(Application $app)
    {
        $this->app = $app;
    }

    /**
     * @return array
     */
    public function generateElasticSearchFilter():array
    {
        /**
         * @var VariationBaseFilter $variationBaseFilter
         */
        $variationBaseFilter = $this->app->make(VariationBaseFilter::class);
        $variationBaseFilter->isActive();
        $searchFilter[] = $variationBaseFilter;

        return $searchFilter;
    }
}
