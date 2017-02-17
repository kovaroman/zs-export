<?php

namespace ElasticExport\Filters;

use Plenty\Modules\DataExchange\Contracts\Filters;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

class ShopzillaDE extends Filters
{
    /*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

    /**
     * Shopzilla constructor.
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ArrayHelper $arrayHelper)
    {
		$this->arrayHelper = $arrayHelper;
    }

    /**
     * Generate filters.
     * @param  array $formatSettings = []
     * @return array
     */
    public function generateFilters(array $formatSettings = []):array
    {
        $settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

		$searchFilter = [
			'variationBase.isActive?' => [],
		];

		return $searchFilter;
    }
}
